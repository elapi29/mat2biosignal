# matlab-eeg-to-csv

C++ extractor for MATLAB-based EEG datasets that exports signals, trial markers, labels, artifact flags, and metadata into flat files for downstream analysis.

## Features

The extractor reads MATLAB EEG session structures and exports:

- `*_Xt_*.csv` - time + signal matrix
- `*_trial_*.csv` - trial markers
- `*_y_*.csv` - class labels
- `*_artifacts_*.csv` - artifact flags
- `*_meta_*.txt` - session metadata

## Files

- `bnci_export.cpp` - main C++ exporter
- `export_all.sh` - helper script to run batch exports
- `docs/pipeline.png` - pipeline overview

## Build

This project uses MATIO to read MATLAB `.mat` files.

Example build command:

g++ -O2 -std=c++17 bnci_export.cpp -lmatio -o bnci_export

## Usage

./bnci_export input.mat output_prefix

Example:

./bnci_export B01E.mat B01E

This produces files such as:

- `B01E_Xt_0.csv`
- `B01E_trial_0.csv`
- `B01E_y_0.csv`
- `B01E_artifacts_0.csv`
- `B01E_meta_0.txt`

## Notes

- Raw datasets are not included in this repository.
- Generated outputs are excluded to keep the repository lightweight.
- The goal is to make MATLAB EEG datasets easier to use in downstream processing pipelines.
