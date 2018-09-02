# shell script for pass1 test

source_dir=../examples
expected_dir=../examples/output
source_files=`find $source_dir -depth 1 -type f -print`

temp_dir=`mktemp -d`

result=0

for source_file in $source_files
do
    base_name=`basename $source_file`
    expected_file=$expected_dir/$base_name.pass1.raw
    expected_text=$temp_dir/$base_name.expected.txt
    output_file=$temp_dir/$base_name.pass1.raw
    output_text=$temp_dir/$base_name.actual.txt
    echo $base_name

    echo -n "  processing..."
    ./pass1 $source_file > $output_file
    result=$?

    if [ $result -eq 0 ]
    then
        echo "OK"
    else
        echo "NG"
        break
    fi

    echo -n "  checking..."
    cat $expected_file | ruby diffutil.rb > $expected_text
    cat $output_file | ruby diffutil.rb > $output_text
    # ignore 'file' node difference
    diff_lines=`diff -u $expected_text $output_text | grep "^[+\-][^+\-]" | grep -v "file" | wc -l`

    if [ $diff_lines -eq 0 ]
    then
        echo "OK"
        diff -u $expected_text $output_text
    else
        echo "NG"
        diff -u $expected_text $output_text
        result=1
        break
    fi
done

rm -rf $temp_dir

if [ $result -eq 0 ]
then
    echo "Done."
fi

exit $result
