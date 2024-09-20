
for f in .pio/libdeps/*; do
    if [ -d "$f" ]; then
        echo Updating $f with local library versions:
        cp -v ../../ustd/*.h $f/ustd/
        cp -v ../../muwerk/*.h $f/muwerk/
        cp -v ../../munet/*.h $f/munet/
        cp -av ../../mupplet-core/src $f/mupplet-core
        cp -av ../../mupplet-sensor/src $f/mupplet-sensor
        cp -av ../../mupplet-display/src $f/mupplet-display
    fi
done

