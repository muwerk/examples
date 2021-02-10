
for f in .pio/libdeps/*; do
    if [ -d "$f" ]; then
        echo Updating $f with local library versions:
        cp -v ../../ustd/*.h $f/ustd/
        cp -v ../../muwerk/*.h $f/muwerk/
        cp -v ../../munet/*.h $f/munet/
    fi
done

