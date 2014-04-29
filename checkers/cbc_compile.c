/**
 * @file /home/ryan/uml/robotics/checkers/cbc_compile.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 17, 2014
 *
 * Links the code on the 
 */

int main() {
  system("/mnt/kiss/bin/g++ -o /mnt/browser/usb/cbc_executable /mnt/browser/usb/cbc_executable.o /mnt/kiss/usercode/lib/libcbc.a /mnt/kiss/usercode/lib/libtrack.a /mnt/kiss/usercode/lib/libshared.a /lib/libstdc++.so /mnt/kiss/lib/libgcc_s.so /mnt/kiss/usercode/lib/init.o -lpthread -lm");

  return 0;
}
