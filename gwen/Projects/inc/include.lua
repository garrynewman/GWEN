
dofile( os.get() .. ".lua" )

function DefineRenderer( name, filetable, definestable )

	project ( "Renderer-"..name )
	files( filetable )
	flags( { "Symbols" } )
	kind( "StaticLib" )
    if ( definestable ) then defines( definestable ) end
	
	configuration( "Release" )
		targetname( "GWEN-Renderer-"..name )
		
	configuration( "Debug" )
		targetname( "GWEND-Renderer-"..name )

end

function DefineSample( name, filetable, linktable, linktabled, definestable )

	if ( linktabled == nil ) then linktabled = linktable end
	
	project( "Sample-" .. name )
	targetdir ( "../bin" )
	
	if ( debugdir) then
		debugdir ( "../bin" )
	end
	
	if ( definestable ) then defines( definestable ) end
	files { filetable }
	
	kind "WindowedApp"
		
	configuration( "Release" )
		targetname( name .. "Sample" )
		links( linktable )
		
	configuration "Debug"
		targetname( name .. "Sample_D" )
		links( linktabled )

end

