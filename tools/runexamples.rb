#!/usr/bin/env ruby
# The following script compiles every script in the examples/ directory and
# reports any compilation errors.

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
