_G.action_buffers = {}

--@class Action
--@field name string
--@field bufname string
--@field description string
--@field filetype string
--@field command string
--@field keybind string|nil

--@type Action[]
local actions = {
	{
		name = "Compile",
		description = "Compile",
		bufname = "build.out",
		filetype = "cpp",
		command = "docker exec -i runes_devcontainer sudo ./scripts/linux/compile.bash",
		keybind = "<F5>",
	},
	{
		name = "Configure",
		description = "Configure",
		bufname = "configure.out",
		filetype = "make",
		command = "docker exec -i runes_devcontainer sudo ./scripts/linux/configure.bash",
		keybind = "<F4>",
	},
	{
		name = "Clean",
		description = "Clean",
		bufname = "clean.out",
		filetype = "make",
		command = "docker exec -i runes_devcontainer sudo rm -rf build",
		keybind = "<F3>",
	},
}

--@param action Action
local make_action_handler = function(action)
	_G.action_buffers[action.name] = nil

	if action.keybind ~= nil then
		vim.keymap.set("n", action.keybind, string.format(":%s<CR>", action.name), { desc = action.description })
	end

	return function()
		local buf = _G.action_buffers[action.name]

		if buf == nil or not vim.api.nvim_buf_is_valid(buf) then
			if buf ~= nil then
				vim.api.nvim_buf_delete(buf, { force = true })
			end

			buf = vim.api.nvim_create_buf(true, false)

			vim.api.nvim_set_option_value("buftype", "nowrite", { buf = buf })
			vim.api.nvim_set_option_value("bufhidden", "hide", { buf = buf })
			vim.api.nvim_set_option_value("swapfile", false, { buf = buf })
			vim.api.nvim_set_option_value("modifiable", false, { buf = buf })
			vim.api.nvim_set_option_value("filetype", action.filetype, { buf = buf })
			vim.api.nvim_buf_set_name(buf, action.bufname)

			-- When buffer is deleted then invalidate the pointer to it.
			vim.api.nvim_create_autocmd("BufDelete", {
				buffer = buf,
				callback = function()
					_G.action_buffers[action.name] = nil
				end,
				once = true,
			})

			_G.action_buffers[action.name] = buf
		end

		local win = vim.fn.bufwinid(buf)
		if win == -1 then
			win = vim.api.nvim_get_current_win()
			if not vim.api.nvim_win_is_valid(win) then
				return
			end
		end

		vim.api.nvim_set_current_win(win)
		vim.api.nvim_win_set_buf(0, buf)

		-- Reset buffer contents
		vim.api.nvim_set_option_value("modifiable", true, { buf = buf })
		vim.api.nvim_buf_set_lines(buf, 0, -1, false, {})
		vim.api.nvim_set_option_value("modifiable", false, { buf = buf })

		local line_count = 0
		local function handle_output(_, data, _)
			if data then
				vim.schedule(function()
					-- Filter out empty lines
					local filtered = {}
					for _, line in ipairs(data) do
						if line ~= "" then
							table.insert(filtered, line)
						end
					end

					vim.api.nvim_set_option_value("modifiable", true, { buf = buf })
					vim.api.nvim_buf_set_lines(buf, line_count, line_count, false, filtered)
					line_count = line_count + #filtered
					vim.api.nvim_set_option_value("modifiable", false, { buf = buf })
				end)
			end
		end

		vim.fn.jobstart(action.command, {
			stdcout_buffered = false,
			stdcerr_buffered = false,
			on_stdout = handle_output,
			on_stderr = handle_output,
		})
	end
end

for _, action in ipairs(actions) do
	vim.api.nvim_create_user_command(action.name, make_action_handler(action), {})
end

return {
	"neovim/nvim-lspconfig",
	opts = {
		servers = {
			clangd = {
				cmd = {
					"docker",
					"exec",
					"-i",
					"runes_devcontainer",
					"clangd",
					"--background-index",
					"--header-insertion=iwyu",
					"--completion-style=detailed",
					"--function-arg-placeholders=1",
					"--fallback-style=llvm",
					"--query-driver=**/gcc,**/g++,**/*-gcc,**/*-g++",
					"--path-mappings=/home/gabriel/Runes=/runes",
				},
			},
		},
	},
}
