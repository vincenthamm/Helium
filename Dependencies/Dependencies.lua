local thisFileLocation = ...
if thisFileLocation == nil then
    thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )

configuration {}

if os.get() == "linux" then
	if os.execute( "dpkg -s libbullet-dev" ) ~= 0 then
		print( "Package libbullet-dev is missing" )
		os.exit( 1 )
	end
else
	project "bullet"
		uuid "23112391-0616-46AF-B0C2-5325E8530FBC"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"bullet/src/",
		}
		files
		{
			"bullet/src/btBulletCollisionCommon.h",
			"bullet/src/btBulletDynamicsCommon.h",
			"bullet/src/LinearMath**.cpp",
			"bullet/src/LinearMath**.h",
			"bullet/src/BulletCollision**.h",
			"bullet/src/BulletCollision**.cpp",
			"bullet/src/BulletDynamics**.h",
			"bullet/src/BulletDynamics**.cpp",
		}
end

if os.get() == "linux" then
	if os.execute( "dpkg -s libfreetype6-dev" ) ~= 0 then
		print( "Package libfreetype6-dev is missing" )
		os.exit( 1 )
	end
else
	project "freetype"
		uuid "53C96BED-38E8-4A1f-81E0-45D09AFD33EB"
		kind "StaticLib"
		language "C"
		defines
		{
			"_LIB",
			"FT2_BUILD_LIBRARY",
		}
		includedirs
		{
			"freetype/include",
		}
		files
		{
			"freetype/src/autofit/autofit.c",
			"freetype/src/bdf/bdf.c",
			"freetype/src/cff/cff.c",
			"freetype/src/base/ftbase.c",
			"freetype/src/base/ftbitmap.c",
			"freetype/src/cache/ftcache.c",
			"freetype/src/base/ftfstype.c",
			"freetype/src/base/ftgasp.c",
			"freetype/src/base/ftglyph.c",
			"freetype/src/gzip/ftgzip.c",
			"freetype/src/base/ftinit.c",
			"freetype/src/lzw/ftlzw.c",
			"freetype/src/base/ftstroke.c",
			"freetype/src/base/ftsystem.c",
			"freetype/src/smooth/smooth.c",
			"freetype/src/base/ftbbox.c",
			"freetype/src/base/ftmm.c",
			"freetype/src/base/ftpfr.c",
			"freetype/src/base/ftsynth.c",
			"freetype/src/base/fttype1.c",
			"freetype/src/base/ftwinfnt.c",
			"freetype/src/base/ftxf86.c",
			"freetype/src/base/ftlcdfil.c",
			"freetype/src/base/ftgxval.c",
			"freetype/src/base/ftotval.c",
			"freetype/src/base/ftpatent.c",
			"freetype/src/pcf/pcf.c",
			"freetype/src/pfr/pfr.c",
			"freetype/src/psaux/psaux.c",
			"freetype/src/pshinter/pshinter.c",
			"freetype/src/psnames/psmodule.c",
			"freetype/src/raster/raster.c",
			"freetype/src/sfnt/sfnt.c",
			"freetype/src/truetype/truetype.c",
			"freetype/src/type1/type1.c",
			"freetype/src/cid/type1cid.c",
			"freetype/src/type42/type42.c",
			"freetype/src/winfonts/winfnt.c",
			"freetype/include/ft2build.h",
			"freetype/include/freetype/config/ftconfig.h",
			"freetype/include/freetype/config/ftheader.h",
			"freetype/include/freetype/config/ftmodule.h",
			"freetype/include/freetype/config/ftoption.h",
			"freetype/include/freetype/config/ftstdlib.h",
		}
		configuration "windows"
			files
			{
				"freetype/builds/win32/ftdebug.c",
			}
end

if os.get() == "linux" then
	if os.execute( "dpkg -s libpng12-dev" ) ~= 0 then
		print( "Package libpng12-dev is missing" )
		os.exit( 1 )
	end
else
	project "libpng"
		uuid "46BA228E-C636-4468-9CBD-7CD4F12FBB33"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"zlib"
		}
		files
		{
			"libpng/*.h",
			"libpng/*.c",
		}
		
		if not os.isfile( "libpng/pnglibconf.h" ) then
			os.copyfile( "libpng/scripts/pnglibconf.h.prebuilt", "libpng/pnglibconf.h" );
		end

		local file = io.open("../.git/modules/Dependencies/libpng/info/exclude", "w");
		file:write("pnglibconf.h\n");
		file:close();
end

if os.get() == "linux" then
	if os.execute( "dpkg -s libnvtt-dev" ) ~= 0 then
		print( "Package libnvtt-dev is missing" )
		os.exit( 1 )
	end
else
	project "nvtt"
		uuid "6753B918-F16E-4C13-8DA7-4F9A6DB58B77"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"nvtt/extern/poshlib",
			"nvtt/src",
			"nvtt/src/nvtt/squish",
		}
		files
		{
			"nvtt/extern/poshlib/*.h",
			"nvtt/extern/poshlib/*.cpp",
			"nvtt/src/nvmath/*.h",
			"nvtt/src/nvmath/*.cpp",
			"nvtt/src/nvcore/*.h",
			"nvtt/src/nvcore/*.cpp",
			"nvtt/src/nvthread/*.h",
			"nvtt/src/nvthread/*.cpp",
			"nvtt/src/nvimage/*.h",
			"nvtt/src/nvimage/*.cpp",
			"nvtt/src/nvtt/*.h",
			"nvtt/src/nvtt/*.cpp",
			"nvtt/src/nvtt/cuda/*.h",
			"nvtt/src/nvtt/cuda/*.cpp",
			"nvtt/src/nvtt/squish/*.h",
			"nvtt/src/nvtt/squish/*.cpp",
		}
		excludes
		{
			"nvtt/src/nvcore/Tokenizer.*",
			"nvtt/src/nvimage/ConeMap.*",
			"nvtt/src/nvimage/KtxFile.*",
			"nvtt/src/nvtt/squish/alpha.*",
			"nvtt/src/nvtt/squish/clusterfit.*",
			"nvtt/src/nvtt/squish/rangefit.*",
			"nvtt/src/nvtt/squish/singlecolourfit.*",
			"nvtt/src/nvtt/squish/singlechannelfit.*",
			"nvtt/src/nvtt/squish/squish.*",
			"nvtt/src/nvtt/CompressorDX11.*",
		}
		
		if _ACTION == "vs2012" or _ACTION == "vs2010" then
			configuration "windows"
				includedirs
				{
					"nvtt/project/vc10",
				}
		elseif _ACTION == "vs2008" then
			configuration "windows"
				includedirs
				{
					"nvtt/project/vc9",
				}
		else
			configuration "macosx"
				includedirs
				{
					"nvtt/project/macosx",
				}
		end

		-- Override inline function expansion and intrinsic function usage settings for Debug builds.
		configuration { "windows", "Debug" }
			buildoptions
			{
				"/Ob2",
				"/Oi",
			}
			flags
			{
				"NoEditAndContinue",  -- Edit and continue support is not compatible with /Ob2
			}

		configuration "macosx"
			includedirs
			{
				"nvtt/project/xcode4",
			}
end

if os.get() == "linux" then
	if os.execute( "dpkg -s libois-dev" ) ~= 0 then
		print( "Package libois-dev is missing" )
		os.exit( 1 )
	end
else
	project "ois"
		uuid "4A37964A-C2F4-4FA7-B744-9C4D292DAA22"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"ois/includes/",
		}
		files
		{
			"ois/src/*.cpp",
		}

		configuration "windows"
			files
			{
				"ois/src/win32/*.cpp",
			}

		configuration "macosx"
			files
			{
				"ois/src/mac/*.cpp"
			}
end

if os.get() == "linux" then
	if os.execute( "dpkg -s zlib1g-dev" ) ~= 0 then
		print( "Package zlib1g-dev is missing" )
		os.exit( 1 )
	end
else
	project "zlib"
		uuid "23112391-0616-46AF-B0C2-5325E8530FBA"
		kind "StaticLib"
		language "C++"
		files
		{
			"zlib/*.h",
			"zlib/*.c",
		}
		excludes
		{
			"zlib/gz*.h",
			"zlib/gz*.c",
			"zlib/minigzip.c",
		}
end

--[[
if table.getn( projects() ) == 0 then
	print( "All dependencies were found in packages!" )
	os.exit( 0 )
end
--]]
