int print_version(){
  printf(
    VERSION_STRING, 
    VERSION_MAJOR, 
    VERSION_MINOR, 
    VERSION_MICRO
  );
  return 0;
}

int print_usage(char *bin){
  printf(USAGE_STRING, bin);
  return 1;
}
