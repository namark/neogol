## Scripts in tools/setup
These scripts are designed to be run from the project root. There are 3 of them:
1. init.sh <br />
Clones the dependencies(git clone) and invokes install.sh
2. update.sh <br />
Updated the dependencies(git pull) and invokes install.sh
2. install.sh <br />
Compiles the dependencies and installs them in project root.


They all accept `make` parameters so you can override variables or specify the number of jobs.
