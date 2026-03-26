# mat2biosignal

Lightweight C++ toolkit for converting MATLAB-based EEG and biosignal datasets into clean, analysis-ready flat files for Python, C++, and ML workflows.

## Why this project exists

Many neuroscience and biosignal workflows still begin with legacy MATLAB `.mat` files that are hard to inspect, parse, and connect to modern analysis pipelines.

This project provides a fast export layer that converts structured MATLAB biosignal data into simple, interoperable outputs such as CSV and TXT, making downstream work easier, faster, and more reproducible.

## What it exports

For each session or recording block, the exporter can generate:

- `*_Xt_*.csv` - time + signal matrix
- `*_trial_*.csv` - trial/event markers
- `*_y_*.csv` - labels or class targets
- `*_artifacts_*.csv` - artifact flags
- `*_meta_*.txt` - metadata and session information

## Typical use cases

- Convert legacy MATLAB EEG datasets into Python-ready files
- Simplify ingestion for preprocessing and feature extraction pipelines
- Standardize labels, event markers, and artifact annotations
- Accelerate prototyping for classification or visualization workflows
- Build a bridge between research datasets and production-friendly tooling

Although the current example focuses on EEG, the same approach can be extended to other MATLAB-based biosignal workflows such as EMG, ECG, EOG, and multimodal recordings.

## Repository contents

- `bnci_export.cpp` - core C++ exporter
- `export_all.sh` - helper script for batch export
- `docs/pipeline.png` - visual overview of the export step

## Quick start

### Build

This project uses MATIO to read MATLAB `.mat` files.

Example build command:

g++ -O2 -std=c++17 bnci_export.cpp -lmatio -o mat2biosignal

### Run

./mat2biosignal input.mat output_prefix

Example:

./mat2biosignal B01E.mat B01E

### Example outputs

Running the exporter produces files such as:

- `B01E_Xt_0.csv`
- `B01E_trial_0.csv`
- `B01E_y_0.csv`
- `B01E_artifacts_0.csv`
- `B01E_meta_0.txt`

## Design goals

- Fast data access before preprocessing
- Simple outputs that work well with Python and C++
- Easy inspection of signals, labels, and metadata
- Lightweight integration into downstream ML workflows
- Cleaner handoff between acquisition formats and analysis code

## Who this is for

This project may be useful for:

- researchers working with EEG or other biosignals
- engineers modernizing MATLAB-based pipelines
- healthtech teams building reproducible analysis workflows
- anyone who needs a fast conversion layer between legacy signal files and modern tooling

## Current scope

The current implementation is centered on exporting MATLAB-based EEG dataset structures into flat files for downstream processing.

It is intentionally lightweight and practical: the goal is to reduce friction between raw structured data and analysis environments such as Python notebooks, ML pipelines, or custom C++ tools.

## Roadmap ideas

Possible future improvements:

- support for additional MATLAB biosignal structures
- cleaner CLI arguments
- CMake build support
- sample input/output examples
- Python wrapper for easier integration
- modular parsers for multiple dataset schemas

## Freelance / collaboration

I’m open to freelance work related to:

- MATLAB to Python/C++ data conversion
- EEG / EMG / ECG / EOG export pipelines
- custom parsers for legacy biosignal datasets
- preprocessing-ready data interfaces
- rapid setup of downstream analysis workflows

If you need help turning MATLAB biosignal data into a cleaner analysis pipeline, feel free to reach out.

## Notes

- Raw datasets are not included in this repository.
- Generated outputs are excluded to keep the repository lightweight.
- This repository is meant to demonstrate the export layer, not the full downstream modeling stack.
