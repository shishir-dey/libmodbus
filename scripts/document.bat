rm -r docs
doxygen Doxyfile
mv docs/html/* docs/
rm -r docs/html
rm -r docs/latex
