# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)

Gem::Specification.new do |s|
  s.name        = "recommendify"
  s.version     = "0.3.6"
  s.date        = Date.today.to_s
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Paul Asmuth"]
  s.email       = ["paul@paulasmuth.com"]
  s.homepage    = "http://github.com/paulasmuth/recommendify"
  s.summary     = %q{ruby/redis based recommendation engine (collaborative filtering)}
  s.description = %q{Recommendify is a distributed, incremental item-based recommendation engine for binary input ratings. It's based on ruby and redis and uses an approach called "Collaborative Filtering"}
  s.licenses    = ["MIT"]

  s.extensions = ['ext/extconf.rb']

  s.add_dependency "redis", ">= 2.2.2"

  s.add_development_dependency "rspec", "~> 2.8.0"

  s.files         = `git ls-files`.split("\n") - [".gitignore", ".rspec", ".travis.yml"]
  s.test_files    = `git ls-files -- spec/*`.split("\n")
  s.require_paths = ["lib"]
end
