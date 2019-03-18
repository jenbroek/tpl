tpl - shell templating utility
==============================
tpl reads from standard input, executing shell commands inside
delimiters (default: '{{' and '}}', but modifiable through config.h).

Installation
------------
Edit config.mk to match your local setup (tpl is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install tpl (if
necessary as root):

	make clean install


Running tpl
-----------
Simply invoke 'tpl', optionally redirecting input/output.
