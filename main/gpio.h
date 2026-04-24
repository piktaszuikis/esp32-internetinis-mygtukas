//"Paspausti" kompiuterio įjungimo mygtuką
void gpio_trigger_pc_power_btn();

//"Paspausti" kompiuterio įjungimo mygtuką ilgai
void gpio_trigger_pc_power_long_btn();

//Indikacinio LED'o valdymas
void gpio_set_online_led(int is_online);

//Ar dega kompo "POWER LED"'as.
int gpio_is_pc_powered();

//Nustato numatytąsias GPIO būsenas
void init_gpio();
