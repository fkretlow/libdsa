for t in `find ./tests/ -type f -name "*_tests" | sort`
do
    $t
done
echo ""
