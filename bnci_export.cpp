#include <matio.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

/* ========== helpers ASCII ========== */

static void write_csv_double_1d(const std::string& outPath,
                                const double* data, size_t n)
{
    FILE* f = std::fopen(outPath.c_str(), "w");
    if (!f) throw std::runtime_error("No se pudo abrir output CSV.");
    for (size_t i = 0; i < n; ++i)
        std::fprintf(f, "%.10g\n", data[i]);
    std::fclose(f);
}

static void write_csv_uint8_1d(const std::string& outPath,
                               const uint8_t* data, size_t n)
{
    FILE* f = std::fopen(outPath.c_str(), "w");
    if (!f) throw std::runtime_error("No se pudo abrir output CSV.");
    for (size_t i = 0; i < n; ++i)
        std::fprintf(f, "%u\n", static_cast<unsigned>(data[i]));
    std::fclose(f);
}

static std::string read_mat_char(matvar_t* v)
{
    if (!v || v->class_type != MAT_C_CHAR || !v->data) return "";
    size_t n = 1;
    for (int i = 0; i < v->rank; ++i) n *= v->dims[i];
    const char* p = static_cast<const char*>(v->data);
    return std::string(p, p + n);
}

/* ========== acceso a campos ========== */

static matvar_t* field(matvar_t* st, const char* name)
{
    if (!st || st->class_type != MAT_C_STRUCT) return nullptr;
    return Mat_VarGetStructFieldByName(st, name, 0);   // primer elemento
}

/* ========== exportar una sesión ========= */

static void export_one_session(matvar_t* st,
                               const std::string& prefix,
                               int idx)
{
    /* ---------- Señal X ---------- */
    matvar_t* Xv = field(st, "X");
    if (!Xv || Xv->class_type != MAT_C_DOUBLE || Xv->rank != 2 || !Xv->data)
        throw std::runtime_error("No pude leer data{idx}.X como double 2D.");

    size_t rows = Xv->dims[0], cols = Xv->dims[1];
    const double* X = static_cast<double*>(Xv->data);

    /* ---------- fs para columna tiempo ---------- */
    double fs = 0.0;
    if (matvar_t* fsv = field(st, "fs");
        fsv && fsv->class_type == MAT_C_DOUBLE && fsv->data)
    {
        fs = static_cast<double*>(fsv->data)[0];
    }
    if (fs <= 0.0) throw std::runtime_error("fs inválido (<=0).");

    /* CSV con tiempo + señal */
    std::string csvTS = prefix + "_Xt_" + std::to_string(idx) + ".csv";
    FILE* fSig = std::fopen(csvTS.c_str(), "w");
    if (!fSig) throw std::runtime_error("No pude abrir CSV tiempo+señal.");

    for (size_t r = 0; r < rows; ++r) {
        double t = static_cast<double>(r) / fs;
        std::fprintf(fSig, "%.8f", t);
        for (size_t c = 0; c < cols; ++c) {
            double v = X[r + c * rows];          // column-major
            std::fprintf(fSig, ",%.10g", v);
        }
        std::fprintf(fSig, "\n");
    }
    std::fclose(fSig);

    /* ---------- trial ---------- */
    if (matvar_t* tv = field(st, "trial");
        tv && tv->class_type == MAT_C_DOUBLE && tv->rank == 2 && tv->data)
    {
        size_t n = tv->dims[0] * tv->dims[1];
        write_csv_double_1d(prefix + "_trial_" + std::to_string(idx) + ".csv",
                            static_cast<double*>(tv->data), n);
    }

    /* ---------- y (labels) ---------- */
    if (matvar_t* yv = field(st, "y");
        yv && yv->class_type == MAT_C_DOUBLE && yv->rank == 2 && yv->data)
    {
        size_t n = yv->dims[0] * yv->dims[1];
        write_csv_double_1d(prefix + "_y_" + std::to_string(idx) + ".csv",
                            static_cast<double*>(yv->data), n);
    }

    /* ---------- artifacts ---------- */
    if (matvar_t* av = field(st, "artifacts");
        av && av->class_type == MAT_C_UINT8 && av->rank == 2 && av->data)
    {
        size_t n = av->dims[0] * av->dims[1];
        write_csv_uint8_1d(prefix + "_artifacts_" + std::to_string(idx) + ".csv",
                           static_cast<uint8_t*>(av->data), n);
    }

    /* ---------- meta ---------- */
    FILE* meta = std::fopen((prefix + "_meta_" + std::to_string(idx) + ".txt").c_str(),"w");
    if (!meta) throw std::runtime_error("No pude abrir meta txt.");

    std::fprintf(meta, "fs=%.10g\n", fs);

    if (matvar_t* agev = field(st, "age");
        agev && agev->class_type == MAT_C_DOUBLE && agev->data)
    {
        double age = static_cast<double*>(agev->data)[0];
        std::fprintf(meta, "age=%.10g\n", age);
    }
    if (matvar_t* gv = field(st, "gender")) {
        std::fprintf(meta, "gender=%s\n", read_mat_char(gv).c_str());
    }
    if (matvar_t* cv = field(st, "classes"); cv && cv->class_type == MAT_C_CELL) {
        size_t nc = 1; for (int i = 0; i < cv->rank; ++i) nc *= cv->dims[i];
        for (size_t i = 0; i < nc; ++i) {
            matvar_t* ci = Mat_VarGetCell(cv, (int)i);
            std::fprintf(meta, "class[%zu]=%s\n", i, read_mat_char(ci).c_str());
        }
    }
    std::fclose(meta);

    std::cout << "OK session " << idx << ": Xt(" << rows << "x" << (cols+1)
              << ") + trial/y/artifacts/meta\n";
}

/* ========== main ========= */

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Uso: bnci_export <input.mat> <output_prefix>\n";
        return 1;
    }
    std::string inPath = argv[1];
    std::string prefix = argv[2];

    mat_t* mat = Mat_Open(inPath.c_str(), MAT_ACC_RDONLY);
    if (!mat) { std::cerr << "No pude abrir " << inPath << "\n"; return 2; }

    matvar_t* data = Mat_VarRead(mat, "data");
    if (!data || data->class_type != MAT_C_CELL) {
        std::cerr << "No encontre 'data' como cell.\n";
        if (data) Mat_VarFree(data);
        Mat_Close(mat); return 3;
    }

    size_t n = data->dims[0] * data->dims[1];
    for (size_t i = 0; i < n; ++i) {
        if (matvar_t* cell = Mat_VarGetCell(data, (int)i);
            cell && cell->class_type == MAT_C_STRUCT)
        {
            try { export_one_session(cell, prefix, (int)i); }
            catch (const std::exception& e) {
                std::cerr << "Error sesión " << i << ": " << e.what() << "\n";
            }
        } else {
            std::cerr << "data{" << i << "} no es struct.\n";
        }
    }

    Mat_VarFree(data);
    Mat_Close(mat);
    return 0;
}