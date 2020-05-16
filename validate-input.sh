### Validates input files [routers.config] and [links.config] to make sure that program it's gonna work

## get declared router ids
cat inputs/routers.config | awk '{print $1}' | sort | uniq >routerIds 
## get specified routers on first columns of links file
cat inputs/links.config | awk '{print $1}' | sort | uniq >column1 
## get specified routers on second columns of links file
cat inputs/links.config | awk '{print $2}' | sort | uniq >column2
## get routers on first column of links that it's not declared no routers 
invalidColum1=$(diff column1 routerIds | grep "<" | sed -e 's/< //')
## get routers on second column of links that it's not declared no routers 
invalidColum2=$(diff column2 routerIds | grep "<" | sed -e 's/< //')
## merge invalid routers
invalids="$invalidColum1 $invalidColum2"
## remove blank lines
invalids=$(echo $invalids | grep -v -e '^$')
## remove temporary created files
rm routerIds column1 column2

if ! [[ -z "$invalids" ]]; then
    echo "file inputs/links.config specifies invalid routers: "
    echo "routers [$invalids] not declared on inputs/routers.config"
    exit 1
fi
