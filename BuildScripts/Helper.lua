-- Function to filter a table
function table.filter(tbl, func)
   local newTbl = {}
   for i, v in ipairs(tbl) do
       if func(v) then
           table.insert(newTbl, v)
       end
   end
   return newTbl
end

-- Function to add files with exclusions to a project
function addFilesWithFilters(includePatterns, excludePatterns)
   local filesToInclude = {}

   -- Get the current project's name
   local currentProjectName = project().name

   -- If includePatterns is empty or nil, skip matching files
   if includePatterns and #includePatterns > 0 then
       -- Process each include pattern
       for _, includePattern in ipairs(includePatterns) do
           local matchedFiles = os.matchfiles(includePattern)
           for _, file in ipairs(matchedFiles) do
               table.insert(filesToInclude, file)
           end
       end
   end

   -- If excludePatterns is provided, apply exclusions
   if excludePatterns and #excludePatterns > 0 then
       for _, excludePattern in ipairs(excludePatterns) do
           filesToInclude = table.filter(filesToInclude, function(file)
               return not string.find(file, excludePattern)
           end)
       end
   end

   -- Add the files to the project only if there are files to include
   if #filesToInclude > 0 then
      print("Adding Files for: " .. currentProjectName)
      files(filesToInclude)

      -- Print out the files found
      for _, file in ipairs(filesToInclude) do
         print("Included file: " .. file)
      end
   else
       print("No files to include for project: " .. currentProjectName)
   end
end