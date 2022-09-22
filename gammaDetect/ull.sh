
for f in .pio/libdeps/*; do
    if [ -d "$f" ]; then
        echo Updating $f with local library versions:
        rm -v $f/ustd/*.h
        cp -v ../../ustd/*.h $f/ustd/
        cp -v ../../muwerk/*.h $f/muwerk/
        cp -v ../../munet/*.h $f/munet/
        cp -v ../../mupplet-core/src/*.h $f/mupplet-core/src
        mkdir -p $f/mupplet-sensor/src
        mkdir -p $f/mupplet-sensor/src/helper
        cp -v ../../mupplet-sensor/src/*.h $f/mupplet-sensor/src
        cp -v ../../mupplet-sensor/src/helper/*.h $f/mupplet-sensor/src/helper
        cp -v ../../mupplet-display/src/*.h $f/mupplet-display/src
    fi
done

