# VESC_6_in_elmoto
A electronics refit project for a electric moped

## Overall design

The dashboard contains a micro controller which is both responsible for driving
the LCD screen, and for communicating the state of the light switches to the
second dairy micro controller under the seat. There are four switches connected
to the dashboard, one for each brake lever and one for the horn and the light
switch. The VESC communicates things like battery voltige and motor rpm to the
dashboard which displays the data in a human readable format on the lcd.
The second dairy microcontroller drives realise which switch on and off the
lights and the horn.

### LCD

The LCD has a polarizing filter built in because of the operation of a LCD,
this polarization however interferes with the polarization of my sunglasses. So
I added a "depolarization filter" to the front of the LCD in the form of
waterproofing the housing.

## thermal design
The aluminium housing of the VESC6 screwed directly to the metal frame of the
scooter in place of the old ESC, with a layer of thermal interface material in
between. Due to the higher efficiency of the new ESC I don't foresee any
thermal constraints.

The heat producing components of the VESC6 are thermally connected to the
aluminium housing. There is a slight inefficiency in his flow because of this
aluminium intermediary, Theoretically the aluminium housing could be removed
and the components mounted directly to the frame, but I see no practical way of
doing this easily.
