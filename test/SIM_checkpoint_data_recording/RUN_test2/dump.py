import trick
from trick.unit_test import *

# This was just here for convenience to dump the checkpoints.

def main():    
    trick.checkpoint(5.0)

    trick.stop(10.0)

if __name__ == "__main__":
    main()