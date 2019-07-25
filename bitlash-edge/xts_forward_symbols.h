#ifndef __xts_forward_symbols_h
#define __xts_forward_symbols_h 1

  typedef unsigned char byte;

  void cmd_function(void);
  void eraseentry(char *id);
  void cmd_peep(void);
  void cmd_ls(void);
  void cmd_help(void);

  void pointToError(void);

  byte findbuiltin(char *name);
  char find_user_function(char *id);

  void displayBanner(void);

  void initlbuf(void);

  void eeinit(void);

#endif