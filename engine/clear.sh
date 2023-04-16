for filename in `ls node_records/*`; do
	rm $filename
done 2> /dev/null

for filename in `ls adj_lists/*`; do
	rm $filename
done 2> /dev/null

for filename in `ls indices/*`; do
	rm $filename
done 2> /dev/null
