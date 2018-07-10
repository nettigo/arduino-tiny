# -*- encoding : utf-8 -*-

require 'rubygems'

require 'json'
require 'pp'
require 'open-uri'


require 'optparse'
require 'ostruct'
options = {}


class Optparse

  CODES = %w[iso-2022-jp shift_jis euc-jp utf8 binary]
  CODE_ALIASES = {"jis" => "iso-2022-jp", "sjis" => "shift_jis"}

  #
  # Return a structure describing the options.
  #
  def self.parse(args)
    # The options specified on the command line will be collected in *options*.
    # We set default values here.
    options = OpenStruct.new
    options.library = []
    options.inplace = false
    options.encoding = "utf8"
    options.transfer_type = :auto
    options.verbose = false

    opt_parser = OptionParser.new do |opts|
      opts.banner = "Usage: package.rb [options]"

      opts.separator ""
      opts.separator "Specific options:"

      opts.on("-f", "--file FILE", 'ZIP file with package') do |o|
        options[:file] = o
      end
      opts.on("-v", "--version V", 'which version we do generate') do |o|
        options[:version] = o
      end
    end

    opt_parser.parse!(args)
    options
  end # parse()

end # class OptparseExample

options = Optparse.parse(ARGV)

raise "Podaj plik z archiwum" if options[:file].nil? || options[:file].empty?
raise "Podaj wersję którą wygenerować" if options[:version].nil? || options[:version].empty?

def get_tool_dependencies(ver = '1.6.11')
  arduino_json_path ="#{ENV['HOME']}/arduino-1.6.9/dist/package_index.json"
  data = File.read(arduino_json_path)

  js= JSON.parse(data)
  package = js["packages"][0]["platforms"].select { |p|
    p['architecture'] == 'avr' && p['version'] == ver
  }.first
  return {} if package.empty?
  # pp package['toolsDependencies']
end

def find_version(current, ver)
  current["packages"][0]["platforms"].each_with_index { |p, i|
    if p['architecture']=='avr' && p['version'] == ver
      return i
    end
  }
  return nil
end

def get_current_index
  JSON.load(open('http://static.nettigo.pl/tinybrd/package_nettigo.pl_index.json'))
end

platform_entry = {
    name: 'Nettigo tinyBrd',
    architecture: 'avr',
    version: options[:version],
    category: 'tinyBrd',
    help: {online: "http://akademia.nettigo.pl/tinybrd"},
    url: "http://static.nettigo.pl/tinybrd/cores/#{options[:file]}",
    archiveFileName: options[:file],
    checksum:
        "SHA-256:" +`sha256sum -b #{options[:file]}`.split()[0],
    size: "#{File.size(options[:file])}",
    boards:
        [
            name: 'tinyBrd'
        ],
    toolsDependencies: get_tool_dependencies

}

packages_data = get_current_index

idx = find_version(packages_data,options[:version])

packages_data ||= {
    packages: [
        name: 'tinybrd',
        maintainer: 'Nettigo',
        websiteURL: 'https://nettigo.pl',
        email: 'info@nettigo.pl',
        help: {online: 'http://akademia.nettigo.pl/tinybrd'},
        platforms: [

        ],
        tools: [],
    ]
}

if idx.nil?
  packages_data["packages"][0]["platforms"] << platform_entry
else
  packages_data["packages"][0]["platforms"][idx] = platform_entry
end

puts JSON.generate(packages_data)


