## Creates one terminal window for each specified router on inputs/routers.config 

OUTPUT1=$(./validate-input.sh)

# find out exit status of validate-input.sh
STATUS1=$?

# ... until now
echo $OUTPUT1

# do something based on the result
if ! [[ $STATUS1 -eq 0 ]]; then 
    exit 1
fi

numberOfRouters=$(cat inputs/routers.config | grep  -v -e '^$' | wc --l | awk '{print $1}')

for refLine in `seq 1 $numberOfRouters`
do
    routerId=$(cat inputs/routers.config | head -n +$refLine | tail -n 1 | awk '{print $1;}')
    ./create-router.sh $routerId
done