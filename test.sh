#!/bin/bash

URL="http://localhost:60223/index.html"
REQUESTS=50

echo "Spawning $REQUESTS concurrent requests to $URL..."

for i in {1..1000}
do
   # The '&' at the end makes it run in the background
   curl -s $URL > /dev/null & 
done

# Wait for all background processes to finish
wait
echo "Done."