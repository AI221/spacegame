cd bin
if [ ! -f Spacegame ]; then
	cd ..
	echo ";;		   Building..."
	./BUILD.sh
	cd bin
fi

echo ";;		   RUNNING"

./Spacegame

cd ..
