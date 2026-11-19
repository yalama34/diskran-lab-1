import os
import subprocess
import sys
import tempfile


def run(executable, commands):
    result = subprocess.run([executable], input="\n".join(commands) + "\n",
                            text=True, capture_output=True, check=True)
    return result.stdout.splitlines()


def word(number):
    result = ""
    while True:
        result += chr(ord("a") + number % 26)
        number //= 26
        if not number:
            return result


with tempfile.TemporaryDirectory() as directory:
    path = os.path.join(directory, "tree file.bin")
    commands = [f"+ {word(i)} {i}" for i in range(5000)]
    commands += [f"! Save {path}", "- a", f"! Load {path}"]
    commands += [word(i).upper() for i in range(5000)]
    expected = ["OK"] * 5003 + [f"OK: {i}" for i in range(5000)]
    assert run(sys.argv[1], commands) == expected
    with open(path, "rb") as source:
        data = source.read()
    for invalid in (data[:16], data[:-1], data + b"x", b"wrong" + data[5:]):
        with open(path, "wb") as target:
            target.write(invalid)
        output = run(sys.argv[1], ["+ keep 9", f"! Load {path}", "keep"])
        assert output[0] == "OK" and output[1].startswith("ERROR:") and output[2] == "OK: 9"

