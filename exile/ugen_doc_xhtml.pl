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
$namespace_name ="";
$export_open = 0;
$export_name ="";
@export_comments = ();
$export_param_num = 0;
$export_is_function = 0;

%library_notes = ( 
    'stk' => '(STK Import)'
    );


$lib_src = "";
$lib_note = "";

$doc_class_name = "";
$doc_info_open = 0;
%doc_brief;
%doc_data;

$example_open = 0;
$example_ready = 0;
@example_lines = ();

sub print_example;
sub print_section;

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

    $outfile = substr ( $file, rindex($file, "/" ) + 1 );
    $outfile =~ s/(\.cpp|\.c)/.html/;
    print "file: $file : generating documentation in $outfile\n";
    open ( HTML , ">" , "$outfile" ) || die "outfile could not be opened";
    print HTML "
<html>
<head>
<title>chuck documentation</title>
<link rel=\"stylesheet\" type=\"text/css\" href=\"ckdoc.css\">
</head>
<body>
<div class=\"library\">
<div class=\"exportlist\">
";

    open ( SOURCE , $file ) || printf " cannot open file \n";
    while ( <SOURCE> ) { 

	if ( /\/\*\! \\example\s*(\w*)/ ) { 
	    $example_open = 1;
	    @example_lines = ();
	}
	elsif ( $example_open ) { 
	    if ( /(.*)\*\// ) { 
		push ( @example_lines, $1 );
		$example_open = 0;
		$example_ready = 1;
		if ( !$export_open && !$ugen_open ) { 
		    print_example();
		}
	    }
	    else { 
		push ( @example_lines, $_ );
	    }

	}

#	if ( /\/\/\!\s*\\(section\w*)\s+(.*)$/ ) {  //in comment case

	if ( /\/\/\!\s*(.*)$/ ) { 
	    #comments
	    my $comm = $1;
	    if ( $comm =~ /\\(section\w*)\s+(.*)/ ) { 
		print_section ( $2, $1 );
	    }
	    else { 
		push @comments, $comm;
	    }
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


	if ( /^\s*QUERY->add_export\s*\(\s*(.*?)\)/ ) { 
	    open_export ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

	if ( /^\s*QUERY->add_param\s*\(\s*(.*?)\)/ ) { 
	    add_param ( split ( /[\s,\"]+/ , $1 ) , @comments);
	    @comments = ();
	}

	#stupid lazy shortcuts in ckx libs
	if ( /^\s*(\w+)_CKADDEXPORT\s*\(\s*(.*?)\)/ ) { 
	    my ( $type, $name ) = split ( /[\s,\"]+/ , $2 );
	    @stdargs = ( "QUERY", $type, $name, lc($1).$name."_impl", "TRUE");
	    open_export ( @stdargs , @comments);
	    @comments = ();
	}

	#more laziness biting me in the ass
	if ( /^\s*(\w+)_CKADDPARAM\s*\(\s*(.*?)\)/ ) { 
	    @expargs = split ( /[\s,\"]+/ , $2 );
	    @stdargs = ( "QUERY", @expargs );
	    add_param ( @stdargs , @comments);
	    @comments = ();
	}


    }
    cleanup();
    close ( SOURCE );
    print HTML "\n</div></div></body></html>";
    close ( HTML );

}



sub open_ugen { 
    my ( $query, $name, $arg2, @comments ) = @_;
    cleanup();
    $ugen_name = $name;

    $brief = $doc_brief{$ugen_name};
    $data = $doc_data{$ugen_name};
    if ( $brief ne "" ) { @comments = ( $brief, @comments ); }


    print HTML "
<div class=\"ugen\">
<h3><a class=\"heading\" name=\"$ugen_name\">[ugen]</a>: <span class=\"name\">$ugen_name</span> $lib_note</h3>
<div class=\"comments\"><ul>
";
    foreach ( @comments ) { print HTML "<li> $_</li>\n"; } 
    print HTML "</ul></div>\n";
    if ( $data ne "" ) { print HTML "<pre class=\"data\">$data</pre>"; }
    $ugen_open = 1;
}

sub close_ugen { 

    if ( $flist_open ) { close_flist(); }
    if ( $example_ready ) { print_example(); }

    print HTML "</div>\n";

    $ugen_name = "";
    $ugen_open = 0;
}

sub print_extends { 
    my ( $query, $parent ) = @_;
    print HTML "<h4>extends <b><a href=\"#$parent\">$parent</a></b></h4>";
}


sub cleanup { 
    if ( $ugen_open == 1) { close_ugen(); }
    if ( $export_open == 1) { close_export(); }
}

sub open_flist { 

    print HTML "
<div class=\"members\">
[ctrl param]:
<ul>";
    $flist_open = 1;
}

sub close_flist { 
    print HTML "</ul></div>\n";
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
    print HTML "<li><b>.$name</b> - ( $type , $access ) - <span class=\"comment\">$comm</span></li>\n";
#    print HTML "---member $type $name $ctor $cget -- $comm\n";


}

sub print_example { 
    print HTML "
<div class=\"example\">[example]<br />
<div class=\"code\">";
    foreach ( @example_lines )  { print HTML substr($_,5);}
    print HTML "</div></div>";
    @example_lines = ();
    $example_ready = 0;
}

sub print_section { 
    cleanup(); #sections end current class...
    my ( $name, $type ) = @_;
    print HTML "<div class=\"$type\">$name</div>";
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

    if ( $flag eq "TRUE" ) { 
	print HTML "<div class=\"export\"><h3>
<a class=\"heading\" name=\"$name\">[function]</a>: 
$type <b class=\"name\">$name</b> ( ";
	$export_is_function = 1;
    }

    else { 
	print HTML "<div class=\"export\"><h3>
<a class=\"heading\" name=\"$name\">[constant]</a>: 
$type <b class=\"name\">$name</b></h3>\n";
	$export_is_function = 0;
    }
    $export_param_num = 0;
    $export_open = 1;
}

sub close_export { 

    if ( $export_is_function == 1 ) { print HTML " );</h3>\n"; }
    if ( scalar( @export_comments ) > 0 ) { 
	print HTML "<div class=\"comments\"><ul>\n";
	foreach ( @export_comments ) { print HTML "<li>$_</li>\n"; } 
	print HTML "</ul></div>\n";
    }
    if ( $example_ready ) { print_example(); }
    print HTML "</div>\n";
    $export_open = 0;


}

sub add_param { 
    my ($query, $type, $name, @comments ) = @_;
    push ( @export_comments, @comments );
    $sep = "";
    if ( $export_param_num ) { $sep = ", "; }
    print HTML "$sep$type <b class=\"arg\">$name</b>";
    $export_param_num ++;
}
