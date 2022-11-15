import os

should_be_quiet = os.getenv('CI') is not None
print ("Should be quiet: ", should_be_quiet)
