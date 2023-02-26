for i in `seq 15`
do
    energy=`expr $i \* 2`
    echo "./pi_macro/pi_${energy}GeV.mac"
    ./exampleB4a -m "./pi_macro/pi_${energy}GeV.mac"
    mv "B4.root" "pi.root"
    mv "pi.root" "../CNN_3cm/test_dataset/data_${energy}GeV"
    python ../messege.py "geant4_${energy}GeV_finish"
done
