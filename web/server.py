#!/usr/bin/env python3
"""
server.py - Local web wrapper for the Mini Language Compiler.

It serves the web page and exposes one endpoint (/compile) that takes the
source code typed in the browser, runs the REAL compiled `./compiler` binary
on it, and returns the compiler's output. No compiler logic lives here; this
only shuttles input to the binary and output back to the browser.
"""

import os
import subprocess
import tempfile
from flask import Flask, request, jsonify, send_from_directory

# Folder this script lives in (web/), and the project root (one level up).
WEB_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(WEB_DIR)
# The compiler executable is built at the project root by `make`.
COMPILER = os.path.join(PROJECT_ROOT, "compiler")

app = Flask(__name__, static_folder=WEB_DIR, static_url_path="")


@app.route("/")
def index():
    # Serve the front-end page.
    return send_from_directory(WEB_DIR, "index.html")


@app.route("/compile", methods=["POST"])
def compile_code():
    data = request.get_json(silent=True) or {}
    source = data.get("code", "")

    # Make sure the compiler has actually been built.
    if not os.path.exists(COMPILER):
        return jsonify({
            "ok": False,
            "output": "Compiler binary not found. Run `make` in the project "
                      "root first to build ./compiler."
        })

    # Write the submitted program to a temporary .mc file.
    try:
        with tempfile.NamedTemporaryFile("w", suffix=".mc", delete=False) as f:
            f.write(source)
            tmp_path = f.name
    except OSError as e:
        return jsonify({"ok": False, "output": f"Could not write temp file: {e}"})

    # Run the real compiler on the temp file, feeding it via stdin
    # (the compiler reads from stdin), with a timeout as a safety net.
    try:
        result = subprocess.run(
            [COMPILER],
            stdin=open(tmp_path, "r"),
            capture_output=True,
            text=True,
            timeout=10,
        )
        # Combine normal output and error output so the user sees everything
        # (syntax/lexical/semantic errors are printed to stderr).
        combined = result.stdout
        if result.stderr:
            combined += "\n" + result.stderr
        return jsonify({"ok": True, "output": combined})
    except subprocess.TimeoutExpired:
        return jsonify({"ok": False, "output": "Compilation timed out (possible infinite loop)."})
    except Exception as e:
        return jsonify({"ok": False, "output": f"Error running compiler: {e}"})
    finally:
        # Clean up the temp file.
        try:
            os.unlink(tmp_path)
        except OSError:
            pass


if __name__ == "__main__":
    # host=127.0.0.1 keeps it local to your machine (safe for a demo).
    print("Mini Compiler web UI running at:  http://127.0.0.1:5000")
    print("Press Ctrl+C to stop.")
    app.run(host="127.0.0.1", port=5000, debug=False)
