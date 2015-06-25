#!/bin/bash

function usage() {
    echo "translate.sh: Prepare translation files for building or editing."
    echo ""
    echo "              Lucy the Diamond Girl uses only certain characters for displaying on screen,"
    echo "              languages with other than characters a-z must remap the other characters"
    echo "              to certain free characters.  For details, see README.translations"
    echo ""
    echo "              Because it's clumsy to write/edit the translations using the mapped"
    echo "              characters directly, this little tool can be used to automatically"
    echo "              convert between the utf-8 and the remapped characters."
    echo ""
    echo "              Use <map> to change the characters from utf-8 before compiling/building."
    echo "              And use <unmap> to change the characters back to utf-8 for editing."
    echo ""
    echo "Usage: ${0} <map|unmap> <language>"
    echo "  map        prepare translation files for building"
    echo "  unmap      prepare translation files for editing"
    echo "  <language> is one of the following:"
    echo "             fi fr"
}

COMMAND=${1}
if [ "${COMMAND}" != "map" -a "${COMMAND}" != "unmap" ]; then
    usage
    exit 1
fi

LANGUAGE=${2}
if [ "${LANGUAGE}" != "fi" -a "${LANGUAGE}" != "fr" ]; then
    usage
    exit 1
fi

SEDFILE=translate-${COMMAND}-${LANGUAGE}.sed

# PO files, create and use a temporary sed script based on the SEDFILE
TMPSEDFILE=translate.sed
if [ -f ${TMPSEDFILE} ]; then
    echo "Error! Temporary file '${TMPSEDFILE}' already exists, not going to overwrite."
    exit 1
fi
echo '/^msgstr "/ {' >  ${TMPSEDFILE}
cat ${SEDFILE}       >> ${TMPSEDFILE}
echo '}'             >> ${TMPSEDFILE}
for FILE in po/${LANGUAGE}.po ; do
    if [ -f ${FILE} ]; then
        echo ${FILE}
        sed --in-place --file=${TMPSEDFILE} ${FILE} || exit 1
    fi
done
rm ${TMPSEDFILE}


# Other files
for FILE in data/*-${LANGUAGE}.txt ; do
    if [ -f ${FILE} ]; then
           echo ${FILE}
           sed --in-place --file=${SEDFILE} > ${FILE} || exit 1
       fi
done


if [ ${COMMAND} == "map" ]; then
    make -j1 -C po update-po
fi
