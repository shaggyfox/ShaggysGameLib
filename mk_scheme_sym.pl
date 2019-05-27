#!/usr/local/bin/perl

$output = pop @ARGV;
open($out, ">", $output);

$fncc = 0;
@mylist = ();

print $out "#include \"engine.h\"\n";
print $out "#include \"scheme_helper.h\"\n\n";

for(@ARGV) {
  open($in, "<", $_);

  while(<$in>) {
    $_ =~ s/\n//;
    if (m/API_CALL [^ ]*( [A-Z_0-9]+)*/) {
      $return_type = "NIL";
      print $out $_ . "\n";
      @words = split(/ /, substr($_, $-[0], $+[0] - $-[0]));
      $i = 0;
      @var = ();
      for(@words) {
        if ($i == 1) {
          $name = $_;
        } elsif ($i != 0) {
          @var[$i - 2] = $_;
        }
        ++$i;
      }
      #define C function
      print $out "static pointer scheme_$name(scheme *sc, pointer args) {\n";
      $i = 0;
      $rval = 0;
      for (@var) {
        if (m/RETURNS_/) {
          ++$rval;
        }
      }
      if (scalar @var - $rval > 0 ){
        print $out "  char *err = NULL;\n";
      }
      for (@var) {
        $no_error_check = 0;
        if ($_ eq "INT") {
          print $out "  int arg$i = scheme_get_integer(sc, &args, &err);\n";
        } elsif ($_ eq "FLOAT") {
          print $out "  float arg$i = scheme_get_float(sc, &args, &err);\n";
        } elsif ($_ eq "STR") {
          print $out "  char *arg$i = scheme_get_string(sc, &args, &err);\n";
        } elsif ($_ eq "RECT") {
          print $out "  SDL_Rect *arg$i = scheme_get_SDL_Rect(sc, &args, &err);\n";
        } elsif ($_ eq "TILESET") {
          print $out "  struct tileset *arg$i = scheme_get_tileset(sc, &args, &err);\n";
        } elsif ($_ eq "FRAME") {
          print $out "  struct frame *arg$i = scheme_get_frame(sc, &args, &err);\n";
        } elsif ($_ eq "MAP") {
          print $out "  struct map *arg$i = scheme_get_map(sc, &args, &err);\n";
        } elsif ($_ eq "ANIMATION") {
          print $out "  struct animation *arg$i = scheme_get_animation(sc, &args, &err);\n"
        } elsif ($_ eq "ANIMATION_CTX") {
          print $out "  struct animation_ctx *arg$i = scheme_get_animation_ctx(sc, &args, &err);\n";
        } elsif ($_ eq "RETURNS_BOOL") {
          pop @var;
          $return_type = "BOOL";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_INT") {
          pop @var;
          $return_type = "INT";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_TILESET") {
          pop @var;
          $return_type = "TILESET";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_NIL") {
          pop @var;
          $return_type = "NIL";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_FRAME") {
          pop @var;
          $return_type = "FRAME";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_ANIMATION") {
          pop @var;
          $return_type = "ANIMATION";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_ANIMATION_CTX") {
          pop @var;
          $return_type = "ANIMATION_CTX";
          $no_error_check = 1;
        } elsif ($_ eq "RETURNS_MAP") {
          pop @var;
          $return_type = "MAP";
          $no_error_check = 1;
        }
        if ($no_error_check eq 0) {
          print $out "  if (err) {printf(\"%s: %s\\n\", \"$name\", err);return sc->NIL;}\n";
        }
        ++$i;
      }
      $i = 0;
      if ($return_type eq "TILESET") {
        print $out "  return scheme_tileset_to_pointer(sc, ";
      } elsif ($return_type eq "FRAME"){
        print $out "  return scheme_frame_to_pointer(sc, ";
      } elsif ($return_type eq "ANIMATION") {
        print $out "  return scheme_animation_to_pointer(sc, ";
      } elsif ($return_type eq "ANIMATION_CTX") {
        print $out "  return scheme_animation_ctx_to_pointer(sc, ";
      } elsif ($return_type eq "INT") {
        print $out "  return scheme_int_to_pointer(sc, ";
      } elsif ($return_type eq "BOOL") {
        print $out "  return scheme_bool_to_pointer(sc, ";
      } elsif ($return_type eq "MAP") {
        print $out "  return scheme_map_to_pointer(sc, ";
      }
      print $out "  $name(";
      for (@var) {
        if ($i != 0) {
          print $out ", ";
        }
        print $out "arg$i";
        ++$i;
      }
      print $out ")";
      if ($return_type eq "NIL") {
        print $out " ;\n return sc->NIL;\n}\n";
      } else {
        print $out ");\n}\n";
      }
      #register
      $s_name = lc $name;
      $s_name =~ s/_/-/g;
      push @mylist, "  FUNCTION(\"$s_name\", scheme_$name);\n";
    }

  }
  close $in;
}

print $out "#define FUNCTION(name, function) \\
  sc->vptr->scheme_define(sc, sc->global_env, \\
  sc->vptr->mk_symbol(sc, name), \\
  sc->vptr->mk_foreign_func(sc, &function))\n\n";

print $out "void scheme_init_api(scheme *sc) {\n";
  print $out @mylist;
  print $out "}\n";
