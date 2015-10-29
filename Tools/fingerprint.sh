#
# Fingerprint for regression testing
#

echo "*** Version:"
echo "${floydVersion}"
echo

echo "*** Fingerprint date:"
date
echo

echo "*** System information:"
uname -a
echo

echo "*** Working directory:"
pwd
echo

echo "*** Last git commit:"
git log -1
echo

echo "*** Git status:"
git status
echo

echo "*** Build:"
make clean floyd module floyd.w32.exe floyd-pgo2
echo

echo "*** Help text:"
echo help | ./floyd
echo

echo "*** Smoke test:"
make easy | grep result
echo bench | ./floyd
echo bench | wine ./floyd.w32.exe
echo

echo "*** Game playing:"
cutechess-cli -concurrency 4 -rounds 50 -repeat -each tc=10+0.15\
	-openings file=Data/book-6000-openings.pgn\
	-engine cmd=./floyd-pgo2 proto=uci\
	-engine cmd=floyd0.6.osx proto=uci
echo

echo "*** Node count and branching factor:"
make nodes
echo

echo "*** Evaluation residual:"
make residual
echo

echo "*** Tactics:"
make wac | grep result
echo

echo "*** Check mate:"
head -100 Data/qmate.epd | time python Tools/epdtest.py 10 | grep result
echo

echo "*** Speed:"
echo bench movetime 10000 | ./floyd-pgo2
echo

echo "*** Todo list:" # xtodo
make todo | wc -l
echo
