# DOOM port

This is a port of the DOOM engine to profanOS.

## building

```
python build.py
```

## running

Copy the `doom.bin` file to the commands directory of your profanOS installation,
```
cp doom.bin ../profanOS/out/zapps/commands/
```

and the core program to the tools directory.
```
cp doom-core.bin ../profanOS/out/zapps/tools/
```

Then, run the following command in the profanOS shell:
```
profanOS [/] -> doom
```
