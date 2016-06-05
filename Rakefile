require "bundler/gem_tasks"
require "rake/extensiontask"
require "rspec/core/rake_task"

Rake::ExtensionTask.new('num_buffer')
RSpec::Core::RakeTask.new(:spec)

task :default => :spec
