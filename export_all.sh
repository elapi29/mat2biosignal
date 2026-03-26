#!/usr/bin/env bash
# export_all.sh  (marca como ejecutable:  chmod +x export_all.sh )

for f in B??E.mat; do      # coincide con B01E.mat ... B09E.mat
  base="${f%.mat}"         # quita la extensión
  echo "Procesando $f ..."
  ./bnci_export "$f" "$base"   # genera base_Xt_0.csv, base_meta_0.txt, etc.
done

echo "✔︎ Conversión terminada"
