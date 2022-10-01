#!/bin/bash 

MAIN_NAME="report"

# Build two times to generate indices, references, citations and all that jazz. 
lualatex -shell-escape -file-line-error $MAIN_NAME.tex && bibtex $MAIN_NAME && \
lualatex -shell-escape -file-line-error $MAIN_NAME.tex && bibtex $MAIN_NAME && \
lualatex -shell-escape -file-line-error $MAIN_NAME.tex && bibtex $MAIN_NAME 

rm -r $MAIN_NAME.aux $MAIN_NAME.bbl $MAIN_NAME.blg $MAIN_NAME.fdb_latexmk $MAIN_NAME.fls $MAIN_NAME.log $MAIN_NAME.out $MAIN_NAME.synctex.gz $MAIN_NAME.toc $MAIN_NAME.lof $MAIN_NAME.lot _minted-$MAIN_NAME $MAIN_NAME.bcf $MAIN_NAME.run.xml
