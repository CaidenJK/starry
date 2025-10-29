#pragma once

namespace Starry {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		virtual void Render() = 0;
		virtual void Update() = 0;
		virtual void Init() = 0;
		virtual void Close() = 0;
	private:
		int64_t m_ID;
	};
}