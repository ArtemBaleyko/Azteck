using Azteck;
using System;

namespace Sandbox
{
	public class Camera : Entity
	{
		public float Speed = 1.0f;

		public float DistanceFromPlayer = 5.0f;

		private Entity _player;

		void OnCreate()
		{
			_player = FindEntityByName("Player");
		}

		void OnUpdate(float ts)
		{
			if (_player != null)
				Translation = new Vector3(_player.Translation.XY, DistanceFromPlayer);

			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.Up))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.Down))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.Left))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.Right))
				velocity.X = 1.0f;

			velocity *= Speed;

			Vector3 translation = Translation;
			translation += velocity * ts;
			Translation = translation;
		}

	}
}
