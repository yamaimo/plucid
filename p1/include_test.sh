# shell script for pass1 test ('include')

exe_dir=`pwd`
source_dir=../examples/include
expected_dir=../examples/output

temp_dir=`mktemp -d`

result=0

base_name=set_main
expected_file=$expected_dir/$base_name.pass1.raw
expected_text=$temp_dir/$base_name.expected.txt
output_file=$temp_dir/$base_name.pass1.raw
output_text=$temp_dir/$base_name.actual.txt
echo $base_name

echo -n "  processing..."
(cd $source_dir; $exe_dir/pass1 $base_name > $output_file)
result=$?

if [ $result -eq 0 ]
then
    echo "OK"
else
    echo "NG"
fi

if [ $result -eq 0 ]
then
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
    fi
fi

rm -rf $temp_dir

if [ $result -eq 0 ]
then
    echo "Done."
fi

exit $result
