
for f in .pio/libdeps/*; do
    if [ -d "$f" ]; then
        echo Updating $f with local library versions:
        rm -v $f/ustd/*.h
        cp -v ../../ustd/*.h $f/ustd/
        cp -v ../../muwerk/*.h $f/muwerk/
        cp -v ../../munet/*.h $f/munet/
        cp -v ../../mupplet-core/src/*.h $f/mupplet-core/src
    fi
done

