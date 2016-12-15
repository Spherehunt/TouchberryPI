#!/usr/bin/env ruby

require 'listen'
require 'net/http'
require 'json'

class Color

	attr_reader :red, :green, :blue

	def initialize 
		@red = 0
		@green = 0
		@blue = 0
	end

	def randomColor
		@red = rand(255)
		@green = rand(255)
		@blue = rand(255)
	end
end

class ThumperRestInterface

	@@DRIVE_SPEED = 70
	@@ID = 'MJ'
	@shift = 0

	def initialize host='http://localhost:3000'
		@host = host
	end

	def strobe
		@color = Color.new
                @color.randomColor
		uri = URI(@host + '/neopixels/effects/strobe/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: @color.red, green: @color.green, blue: @color.blue, delay: 100, id: @@ID }.to_json
		send_request uri, req
	end

	def randomshift
		if @shift == 0
			@color = Color.new
			@color.randomColor
                	uri = URI(@host + '/neopixels/effects/shift/0')
                	req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                	req.body = {red: @color.red, green: @color.green, blue: @color.blue, delay: 50, groupsize: 4, id: @@ID }.to_json
                	send_request uri, req
			@shift = 1
		end
        end

	def dim
		uri = URI(@host + '/neopixels/strings/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 0, green: 0, blue: 0, id: @@ID }.to_json
		send_request uri, req
	end

	def alarm
                uri = URI(@host + '/alarm')
                req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                req.body = {action: "on", id: @@ID }.to_json
                send_request uri, req
		strobe
        end


	def left
		drive @@DRIVE_SPEED, -@@DRIVE_SPEED
	end

	def right
		drive -@@DRIVE_SPEED, @@DRIVE_SPEED
	end

	def forward
		drive @@DRIVE_SPEED, @@DRIVE_SPEED
	end

	def reverse
		drive -@@DRIVE_SPEED, -@@DRIVE_SPEED
	end

	def stop
		drive 0, 0
		uri = URI(@host + '/alarm')
                req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                req.body = {action: "off", id: @@ID }.to_json
                send_request uri, req
		@shift = 0
		dim
	end

	def drive leftspeed, rightspeed
		uri = URI(@host + '/speed')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {left_speed: leftspeed, right_speed: rightspeed, id: @@ID }.to_json
		send_request uri, req
		if @shift == 0
			randomshift
			@shift = 1
		end
	end

	def send_request uri, req
		res = Net::HTTP.start(uri.hostname, uri.port) do |http|
			http.request(req)
		end
	end
end

thumper = ThumperRestInterface.new "http://10.182.34.107:3000"

listener = Listen.to('/tmp/touch') do |modified|
  puts "modified absolute path: #{modified}"
	File.readlines(modified.first).each do |instruction|
		instruction.strip!

		if thumper.respond_to?(instruction.to_sym)
			thumper.send instruction
		else
			thumper.stop
		end

	end
end
listener.start # not blocking

sleep
