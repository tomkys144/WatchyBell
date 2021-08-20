import os

dirname = os.path.dirname(__file__)

with open(os.path.join(dirname, '../README.md'), 'r') as f:
    readme = f.read()
    f.close()
    
readme = readme.replace("WatchyBell", "%WatchyBell", 1)
readme = readme.replace("https://tomkys144.github.io/WatchyBell", "https://github.com/tomkys144/WatchyBell")

with open(os.path.join(dirname, "index.md"), 'w') as f_out:
    f_out.write(readme)
    f_out.close()
