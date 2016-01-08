--[[
	Gizmo Firmware - Lua scripts to support the Gizmo plugin for X-Plane.
    Copyright (C) 2010,2011  Ben Russell, br@x-plugins.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--]]


--[[

Lua CHROOT for Gizmo.
2010-12-04 - Ben Russell - br@x-plugins.com

This script uses a closure to overide and sandbox the Lua io.open function.

IO operations are contained to X-Planes folder and within. No outside ops allowed.


--]]
	do
	
		--Functions in this table are Jailed and confined to inside the X-Plane folder.
		local gizmo_jail_f = {
			io_open = io.open,
			os_remove = os.remove,
			os_rename = os.rename,
			}
		
		
		
		
		--stored at Lua startup to avoid any form of xp table meddling hacks.
		local gizmo_jail_chroot_path = xp.getXPlaneFolder()
		
		
		
		
		local gizmo_jail_function_disabled = function(msg)
			logging.debug(string.format("ACCESS DENIED: Sorry %s is disabled for security reasons.", msg))
		end
		
		os.execute = 	function() gizmo_jail_function_disabled("os.execute") end
		os.tmpname = 	function() gizmo_jail_function_disabled("os.tmpname") end
		os.exit =  		function() gizmo_jail_function_disabled("os.exit") end
		io.popen = 		function() gizmo_jail_function_disabled("io.popen") end

		
		


		---Sandboxed os.rename function
		os.rename = function( old_filename, new_filename )
			logging.debug(string.format('Secured IO: os.rename: (%s) -> (%s)', old_filename, new_filename))
			
			
			local security_score = 0
			
			--first we security check the old filename			
					-- cleansing, removes double dots and other junk
						local old_strlenbefore=string.len(old_filename)+1
						while ( old_strlenbefore>string.len(old_filename) ) do
							old_strlenbefore=string.len(old_filename)
							-- Clean on windows from the ...
							old_filename=string.gsub(old_filename,[[([^%/]+)%\([^%/]+)%\%.%.]],"%1")
							--Clean on linux from the ...
							old_filename=string.gsub(old_filename,[[([^%/]-)%/%.%.]],"")
						end
					
					local old_base_folder = string.sub( old_filename, 1, #gizmo_jail_chroot_path )
					local old_sub_filename = string.sub( old_filename, #gizmo_jail_chroot_path+1 )
					
			
					if( old_base_folder == gizmo_jail_chroot_path )then
						logging.debug( string.format('Secured IO: Approved safe filename for os.rename from: (%s)', old_sub_filename ) )
					
						if( old_sub_filename )then				
							security_score = security_score + 1
						else
							logging.debug('Secured IO: ACCESS DENIED: os.rename: no filename, folder only.')
							return nil,'Access Denied: No file specified.'
						
						end --end check to ensure we got a user component to the path and a base folder
					else
						logging.debug('Secured IO: ACCESS DENIED: os.rename: You may NOT modify files outside the X-Plane folder.')
						return nil,'Access Denied: File outside X-Plane folder.'
					
					end --check to ensure the user is requesting a file inside the base path
		
		
		
			--second we security check the new filename			
					-- cleansing, removes double dots and other junk
						local new_strlenbefore=string.len(new_filename)+1
						while ( new_strlenbefore>string.len(new_filename) ) do
							new_strlenbefore=string.len(new_filename)
							-- Clean on windows from the ...
							new_filename=string.gsub(new_filename,[[([^%/]+)%\([^%/]+)%\%.%.]],"%1")
							--Clean on linux from the ...
							new_filename=string.gsub(new_filename,[[([^%/]-)%/%.%.]],"")
						end
					
					local new_base_folder = string.sub( new_filename, 1, #gizmo_jail_chroot_path )
					local new_sub_filename = string.sub( new_filename, #gizmo_jail_chroot_path+1 )
					
			
					if( new_base_folder == gizmo_jail_chroot_path )then
						logging.debug( string.format('Secured IO: Approved safe filename for os.rename to: (%s)', new_sub_filename ) )
					
						if( new_sub_filename )then				
							security_score = security_score + 1
						else
							logging.debug('Secured IO: ACCESS DENIED: os.rename: no filename, folder only.')
							return nil,'Access Denied: No file specified.'
						
						end --end check to ensure we got a user component to the path and a base folder
					else
						logging.debug('Secured IO: ACCESS DENIED: os.rename: You may NOT modify files outside the X-Plane folder.')
						return nil,'Access Denied: File outside X-Plane folder.'
					
					end --check to ensure the user is requesting a file inside the base path
		
		
		
				
			if( security_score == 2 )then
				return gizmo_jail_f.os_rename( old_filename, new_filename )
			end
		
		
			logging.debug('Secured IO: ACCESS DENIED: os.rename: Unexpected error.')
			return nil,'Access Denied: Unexpected error.'
			
		end --end of os.rename sandboxing






		
		---Sandboxed os.remove function
		os.remove = function( filename )
			logging.debug(string.format('Secured IO: os.remove: %s', filename))
			
			-- cleansing, removes double dots and other junk
				local strlenbefore=string.len(filename)+1
				while ( strlenbefore>string.len(filename) ) do
					strlenbefore=string.len(filename)
					-- Clean on windows from the ...
					filename=string.gsub(filename,[[([^%/]+)%\([^%/]+)%\%.%.]],"%1")
					--Clean on linux from the ...
					filename=string.gsub(filename,[[([^%/]-)%/%.%.]],"")
				end
			
			
			
			local base_folder = string.sub( filename, 1, #gizmo_jail_chroot_path )
			local sub_filename = string.sub( filename, #gizmo_jail_chroot_path+1 )
			
			
			if( base_folder == gizmo_jail_chroot_path )then
				logging.debug( string.format('Secured IO: Approved safe filename for os.remove: (%s)', sub_filename ) )
			
				if( sub_filename )then				
					return gizmo_jail_f.os_remove( filename )
				else
					logging.debug('Secured IO: ACCESS DENIED: os.remove: no filename, folder only.')
					return nil,'Access Denied: No file specified.'
				
				end --end check to ensure we got a user component to the path and a base folder
			else
				logging.debug('Secured IO: ACCESS DENIED: os.remove: You may NOT modify files outside the X-Plane folder.')
				return nil,'Access Denied: File outside X-Plane folder.'
			
			end --check to ensure the user is requesting a file inside the base path
		
		end --end of os.remove sandboxing
		
		
		
		
		
		
		--- Sandboxed io.open function
		io.open = function( filename, mode )
			logging.debug(string.format('Secured IO: io.open: %s / %s', filename, mode))
			
			-- cleansing, removes double dots and other junk
				local strlenbefore=string.len(filename)+1
				while ( strlenbefore>string.len(filename) ) do
					strlenbefore=string.len(filename)
					-- Clean on windows from the ...
					filename=string.gsub(filename,[[([^%/]+)%\([^%/]+)%\%.%.]],"%1")
					--Clean on linux from the ...
					filename=string.gsub(filename,[[([^%/]-)%/%.%.]],"")
				end
			
			
			
			local base_folder = string.sub( filename, 1, #gizmo_jail_chroot_path )
			local sub_filename = string.sub( filename, #gizmo_jail_chroot_path+1 )
			
			
			if( base_folder == gizmo_jail_chroot_path )then
				logging.debug( string.format('Secured IO: Approved safe filename for io.open: (%s)', sub_filename ) )
			
				if( sub_filename )then				
					return gizmo_jail_f.io_open( filename, mode )
				else
					logging.debug('Secured IO: ACCESS DENIED: io.open: no filename, folder only.')
					return nil,'Access Denied: No file specified.'
				
				end --end check to ensure we got a user component to the path and a base folder
			else
				logging.debug('Secured IO: ACCESS DENIED: io.open: You may NOT open files outside the X-Plane folder.')
				return nil,'Access Denied: File outside X-Plane folder.'
			
			end --check to ensure the user is requesting a file inside the base path
		
		end --end of io.open sandboxing
		
		
		
		logging.debug("Disabled: os.execute, os.tmpname, os.exit, io.popen")
		logging.debug("Sandboxed: io.open, os.remove, os.rename")
		--logging.debug(string.format('Confining Lua IO to folder: (%s)',gizmo_jail_chroot_path))
				
		
	end --end of closure that makes it all secure.

