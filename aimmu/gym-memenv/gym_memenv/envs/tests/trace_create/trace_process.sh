rename 's/pim_/pim-/g' *pim_* #renaming by replacing pim_ by pim-
sed -i '1d;$d' *.op #deletes the first and last line in the file as they are incomplet sometimes
