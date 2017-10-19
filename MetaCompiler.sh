ReferenceCounter=0
FilesUnChanged=1

for item in $(find . -name "*.h" -or -name "*.cpp" ); do
	if [ "$item" != './debug_utils.h' ]; then
		TimeBlockCount=$(grep  "TIMED_BLOCK(" $item | wc -l)
		NameBlockCount=$(grep  "NAMED_BLOCK(" $item | wc -l)
		
		GrepResult=$(grep -n "NAMED_BLOCK(\|TIMED_BLOCK(" $item)
		IFS=$'\n'
		for entry in $GrepResult; do
			line=$(echo $entry | awk -F":" '{print$1}')

			NewEntry="${entry%,*}"
			NewEntry=$(echo $NewEntry | awk -F":" '{print$2}')
			NewEntry="${NewEntry}, ${ReferenceCounter});"

			if [ "$entry" != "${line}:${NewEntry}" ]; then
				FilesUnChanged=0		
				echo "Updating: $NewEntry"
				sed -i "${line}s/*/${NewEntry} /" $item
			fi
			ReferenceCounter=$((ReferenceCounter+1))
		done
		unset IFS

	fi
done

DebugRecordCountEntry=$(grep -n "const uint32 kTotalRecordCount =" debug_utils.h)
line=$(echo $DebugRecordCountEntry | awk -F":" '{print$1}')

NewEntry="const uint32 kTotalRecordCount = $ReferenceCounter;"

if [ "$DebugRecordCountEntry" != "${line}:${NewEntry}" ]; then
	FilesUnChanged=0
	echo "Updating: $NewEntry"
	sed -i "/const uint32 kTotalRecordCount =/c\\${NewEntry}" debug_utils.h
fi

if [ $FilesUnChanged = 1 ]; then
	echo "Files up to date"
fi
