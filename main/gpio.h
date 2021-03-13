//"Paspausti" kompiuterio įjungimo mygtuką
void gpio_trigger_pc_power_btn();

//Indikacinio LED'o valdymas
void gpio_set_online_led(int is_online);

//Ar dega kompo "POWER LED"'as.
int gpio_is_pc_powered();

//Nustato numatytąsias GPIO būsenas
void init_gpio();
