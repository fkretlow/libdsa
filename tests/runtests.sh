echo "\nRUNNING TESTS"

for t in `find ./tests/ -type f -name "*_tests"`
do
    echo ">" $t
    $t
    echo "<"
done

echo ""
