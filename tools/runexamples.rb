#!/usr/bin/env ruby
# This script searches for .lla source files in the examples/directory and
# compiles each one separately, reporting any compilation errors each time.

Dir.chdir("..") if Dir.pwd =~ /tools$/

Dir.foreach("examples") do |example|
  next if /\.lla$/ !~ example

  puts "Running #{example}..."
  `./llama examples/#{example}`
  if $?.to_i != 0
    puts "Example file #{example} produced a compilation error!"
    exit 1
  end
end

puts "Everything is OK!"
