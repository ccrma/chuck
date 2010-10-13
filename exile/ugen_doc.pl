#! /usr/bin/perl


sub open_ugen;
sub close_ugen;
sub open_flist;
sub close_flist;
sub print_extends;
sub print_function;
sub cleanup;

$ugen_open = 0;
$flist_open = 0;
$ugen_name = "";

sub open_namespace;
sub close_namespace;
sub open_export;
sub close_export;
sub add_param;

$namespace_open = 0;
$export_open = 0;
$namespace_name ="";
$export_name ="";
@export_comments = ();
$export_param_num = 0;
%library_notes = ( 
    'stk' => '(STK Import)'
    );

$lib_src = "";
$lib_note = "";

$doc_class_name = "";
$doc_info_open = 0;
%doc_brief;
%doc_data;

foreach ( @ARGV ) { 

    $file = $_;

    open ( SOURCE , $file ) || printf " cannot open file \n";
    while ( <SOURCE> ) { 

	if ( /\/\*\! \\class\s*(\w*)/ ) { 
	    $doc_info_open = 1;
	    $doc_class_name = $1;
	    #print "dox for $1\n";
	    $doc_brief{$doc_class_name} = "";
	    $doc_data{$doc_class_name} = "";
	}
	elsif ( $doc_info_open ) { 
	    if ( /(.*)\*\// ) { 
		$doc_data{$doc_class_name} .= $1;
		#print "dox brief $doc_class_name - ".$doc_brief{$doc_class_name}."\n";
		#print "dox data $doc_class_name - ".$doc_data{$doc_class_name}."\n";
		$doc_info_open = 0;
		$doc_class_name = "";
	    }
	    elsif ( /\\brief (.*)/ ) { 
		$doc_brief{$doc_class_name} = $1;
	    }
	    else { 
		$doc_data{$doc_class_name} .= $_;
	    }

	}
    }
    close ( SOURCE );
    open ( SOURCE , $file ) || printf " cannot open file \n";
    while ( <SOURCE> ) { 

	if ( /\/\/\!(.*)$/ ) { 
#	    print "comment - $1\n";
	    push @comments, $1;
	}

	if ( /DLL_QUERY\s+(\w*)_query/ ) { 
	    $lib_src = $1;
	    $lib_note = $library_notes{$1};
	    @comments = ();
	}
	if ( /QUERY->ugen_add\s*\(\s*(.*?)\)/ ) { 
	    open_ugen ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}
	if ( /QUERY->ugen_extends\s*\(\s*(.*?)\)/ ) { 
	    print_extends ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}
	if ( /QUERY->ugen_ctrl\s*\(\s*(.*?)\)/ ) { 
	    print_function ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

	if ( /QUERY->set_name\s*\(\s*(.*?)\)/ ) { 
	    open_namespace ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

	if ( /QUERY->add_export\s*\(\s*(.*?)\)/ ) { 
	    open_export ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

	if ( /QUERY->add_param\s*\(\s*(.*?)\)/ ) { 
	    add_param ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

    }
    cleanup();
    close ( SOURCE );
}



sub open_ugen { 
    my ( $query, $name, $arg2, @comments ) = @_;
    cleanup();
    $ugen_name = $name;

    $brief = $doc_brief{$ugen_name};
    $data = $doc_data{$ugen_name};
    if ( $brief ne "" ) { @comments = ( $brief, @comments ); }


    print "
<br>
<a name=\"$ugen_name\"> </a>
<table width=100% border=0 bgcolor=d9d9d9 cellpadding=1 cellspacing=0>
<tr><td><table width=100% border=0 bgcolor=fdfdfd cellpadding=1 cellspacing=2>
<tr><td align=left><font face=verdana size=2>
<font color=#339933>[ugen]</font>: <b>$ugen_name</b> $lib_note<br>
";
    foreach ( @comments ) { print "- $_<br />\n"; } 
    if ( $data ne "" ) { print "<pre>$data</pre>"; }
    $ugen_open = 1;
}

sub close_ugen { 
    if ( $flist_open ) { close_flist(); }

    print "</td></tr></table></td></tr></table>\n\n";

#    printf "-end $ugen_name\n";

    $ugen_name = "";
    $ugen_open = 0;
}

sub print_extends { 
    my ( $query, $parent ) = @_;
    print "extends <b><a href=\"#$parent\">$parent</a></b><br />";
}


sub cleanup { 
    if ( $ugen_open ) { close_ugen(); }
    if ( $export_open ) { close_export(); }
}

sub open_flist { 

    print "<br>
[ctrl param]: 
<ul>\n\n";
    $flist_open = 1;
}

sub close_flist { 

    print "\n</ul>\n";
    $flist_open = 0;
}

sub print_function { 
    my ($query, $ctor, $cget, $type, $name, @comments) = @_;
    if ( !$flist_open ) { open_flist(); }

    $comm = join ( "<br />", @comments);
    $access = "";

    if ( $ctor ne "NULL" && $cget ne "NULL" ) { $access = "READ/WRITE"; }
    elsif ( $ctor ne "NULL" ) { $access = "WRITE only" }
    elsif ( $cget ne "NULL" ) { $access = "READ only" }
    print "<b>.$name</b> - ( $type , $access ) - $comm<br/>\n";
#    print "---member $type $name $ctor $cget -- $comm\n";


}


sub open_namespace { 
    my ( $query, $name ) = @_;
    $namespace_open = 1;
}

sub close_namespace { 
    $namespace_open = 0;
}

sub open_export { 
    my ( $query, $type, $name, $func, $flag, @comments ) = @_;
    cleanup();
    @export_comments=@comments;
    
    print "[function]: $type <b>$name</b>( ";
    $export_param_num = 0;
    $export_open = 1;
}

sub close_export { 

    print " );<br/>\n";
    foreach ( @export_comments ) { print "- $_<br />\n"; } 
    $export_open = 0;
}

sub add_param { 
    my ($query, $type, $name, @comments ) = @_;
    push ( @export_comments, @comments );
    $sep = "";
    if ( $export_param_num ) { $sep = ", "; }
    print "$sep$type $name";
    $export_param_num ++;
}
