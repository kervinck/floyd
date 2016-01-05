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

echo "*** Todo list:" # xtodo
make todo | wc -l # xtodo
echo

echo "*** Line count:"
CountLoc -v -np Source # http://talkchess.com/forum/viewtopic.php?t=56201
echo

echo "*** Build:"
make clean
make floyd
make module
make floyd-pgo2
make floyd.w32.exe
echo

echo "*** Help text:"
echo help | ./floyd
echo

echo "*** Smoke tests:"
make easy | grep result
./floyd bench quit
echo bench | wine ./floyd.w32.exe
echo

echo "*** Node count and branching factor:"
make nodes
echo

echo "*** Evaluation residual:"
make residual
echo

echo "*** Game playing:"
cutechess-cli -concurrency 4 -rounds 50 -repeat -each tc=10+0.15\
	-openings file=Data/book-6000-openings.pgn order=random\
	-engine cmd=./floyd-pgo2 proto=uci\
	-engine cmd=floyd0.7 proto=uci # user must have this version installed
echo

echo "*** Tactics:"
make wac | grep result
echo

echo "*** Strategy:"
make sts
echo

echo "*** Check mate:"
head -100 Data/qmate.epd | time python Tools/epdtest.py 10 | grep result
echo

echo "*** Speed:"
echo bench movetime 10000 | ./floyd-pgo2
echo
